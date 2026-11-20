.class public final synthetic Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Ljava/lang/Runnable;


# instance fields
.field public final synthetic f$0:Lcom/racas/black/BundleActivity$1;

.field public final synthetic f$1:Ljava/lang/Exception;

.field public final synthetic f$2:Landroid/widget/Button;


# direct methods
.method public synthetic constructor <init>(Lcom/racas/black/BundleActivity$1;Ljava/lang/Exception;Landroid/widget/Button;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$0:Lcom/racas/black/BundleActivity$1;

    iput-object p2, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$1:Ljava/lang/Exception;

    iput-object p3, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$2:Landroid/widget/Button;

    return-void
.end method


# virtual methods
.method public final run()V
    .locals 3

    .line 0
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$0:Lcom/racas/black/BundleActivity$1;

    iget-object v1, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$1:Ljava/lang/Exception;

    iget-object v2, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda2;->f$2:Landroid/widget/Button;

    invoke-virtual {v0, v1, v2}, Lcom/racas/black/BundleActivity$1;->lambda$run$2$com-racas-black-BundleActivity$1(Ljava/lang/Exception;Landroid/widget/Button;)V

    return-void
.end method
