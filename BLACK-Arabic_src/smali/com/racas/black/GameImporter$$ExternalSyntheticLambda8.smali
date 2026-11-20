.class public final synthetic Lcom/racas/black/GameImporter$$ExternalSyntheticLambda8;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Landroid/content/DialogInterface$OnDismissListener;


# instance fields
.field public final synthetic f$0:Ljava/lang/Runnable;


# direct methods
.method public synthetic constructor <init>(Ljava/lang/Runnable;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda8;->f$0:Ljava/lang/Runnable;

    return-void
.end method


# virtual methods
.method public final onDismiss(Landroid/content/DialogInterface;)V
    .locals 0

    .line 0
    iget-object p0, p0, Lcom/racas/black/GameImporter$$ExternalSyntheticLambda8;->f$0:Ljava/lang/Runnable;

    invoke-static {p0, p1}, Lcom/racas/black/GameImporter;->lambda$message$8(Ljava/lang/Runnable;Landroid/content/DialogInterface;)V

    return-void
.end method
